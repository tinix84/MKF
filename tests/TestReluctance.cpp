#include <UnitTest++.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Reluctance.h"
#include "Constants.h"


SUITE(Reluctance)
{
    std::map<OpenMagnetics::ReluctanceModels, double> maximumErrorReluctances = {
        {OpenMagnetics::ReluctanceModels::ZHANG, 0.26},
        {OpenMagnetics::ReluctanceModels::MUEHLETHALER, 0.41},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, 0.42},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, 0.42},
        {OpenMagnetics::ReluctanceModels::MCLYMAN, 0.32},
        {OpenMagnetics::ReluctanceModels::STENGLEIN, 0.36},
        {OpenMagnetics::ReluctanceModels::CLASSIC, 0.81},
    };
    std::map<OpenMagnetics::ReluctanceModels, std::vector<double>> testAverageErrors = {
        {OpenMagnetics::ReluctanceModels::ZHANG, {}},
        {OpenMagnetics::ReluctanceModels::MUEHLETHALER, {}},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, {}},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, {}},
        {OpenMagnetics::ReluctanceModels::MCLYMAN, {}},
        {OpenMagnetics::ReluctanceModels::STENGLEIN, {}},
        {OpenMagnetics::ReluctanceModels::CLASSIC, {}},
    };
    std::map<OpenMagnetics::ReluctanceModels, std::vector<std::pair<json, double>>> testErrorVersusC1 = {
        {OpenMagnetics::ReluctanceModels::ZHANG, {}},
        {OpenMagnetics::ReluctanceModels::MUEHLETHALER, {}},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, {}},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, {}},
        {OpenMagnetics::ReluctanceModels::MCLYMAN, {}},
        {OpenMagnetics::ReluctanceModels::STENGLEIN, {}},
        {OpenMagnetics::ReluctanceModels::CLASSIC, {}},
    };
    std::map<OpenMagnetics::ReluctanceModels, double> maximumErrorEnergies = {
        {OpenMagnetics::ReluctanceModels::ZHANG, 0.1},
        {OpenMagnetics::ReluctanceModels::MUEHLETHALER, 0.23},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, 0.13},
        {OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, 0.13},
        {OpenMagnetics::ReluctanceModels::MCLYMAN, 0.1},
        {OpenMagnetics::ReluctanceModels::STENGLEIN, 0.7},
        {OpenMagnetics::ReluctanceModels::CLASSIC, 1},
    };

    OpenMagnetics::Core get_core(std::string shapeName, json basicGapping, int numberStacks = 1) {
        auto coreJson = json();

        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "GapReluctanceTest";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N87";
        coreJson["functionalDescription"]["shape"] = shapeName;
        coreJson["functionalDescription"]["gapping"] = basicGapping;
        coreJson["functionalDescription"]["numberStacks"] = numberStacks;

        OpenMagnetics::Core core(coreJson);

        return core;
    }

    json get_grinded_gap(double gapLength) {
        auto constants = OpenMagnetics::Constants();
        auto basicGapping = json::array();
        auto basicCentralGap = json();
        basicCentralGap["type"] = "subtractive";
        basicCentralGap["length"] = gapLength;
        auto basicLateralGap = json();
        basicLateralGap["type"] = "residual";
        basicLateralGap["length"] = constants.residualGap;
        basicGapping.push_back(basicCentralGap);
        basicGapping.push_back(basicLateralGap);
        basicGapping.push_back(basicLateralGap);

        return basicGapping;
    }

    json get_distributed_gap(double gapLength, int numberGaps) {
        auto constants = OpenMagnetics::Constants();
        auto basicGapping = json::array();
        auto basicCentralGap = json();
        basicCentralGap["type"] = "subtractive";
        basicCentralGap["length"] = gapLength;
        auto basicLateralGap = json();
        basicLateralGap["type"] = "residual";
        basicLateralGap["length"] = constants.residualGap;
        for (int i = 0; i < numberGaps; ++i)
        {
            basicGapping.push_back(basicCentralGap);
        }
        basicGapping.push_back(basicLateralGap);
        basicGapping.push_back(basicLateralGap);

        return basicGapping;
    }

    json get_spacer_gap(double gapLength) {
        auto basicGapping = json::array();
        auto basicSpacerGap = json();
        basicSpacerGap["type"] = "additive";
        basicSpacerGap["length"] = gapLength;
        basicGapping.push_back(basicSpacerGap);
        basicGapping.push_back(basicSpacerGap);
        basicGapping.push_back(basicSpacerGap);

        return basicGapping;
    }

    double run_test_reluctance(OpenMagnetics::ReluctanceModels modelName, std::string shapeName, json basicGapping, double expectedReluctance) {

        double maximumErrorReluctance = maximumErrorReluctances[modelName];
        auto core = get_core(shapeName, basicGapping);
        auto gapping = core.get_functional_description().get_gapping();
        double calculatedReluctance = 0;
        double calculatedCentralReluctance = 0;
        double calculatedLateralReluctance = 0;
        auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory(modelName);
        auto coreReluctance = reluctanceModel->get_core_reluctance(core);

        for (const auto& gap: gapping) {
            auto result = reluctanceModel->get_gap_reluctance(gap);
            auto gapColumn = core.find_closest_column_by_coordinates(*gap.get_coordinates());
            if (gapColumn.get_type() == OpenMagnetics::ColumnType::LATERAL) {
                calculatedLateralReluctance += 1 / result["reluctance"];
            }
            else {
                calculatedCentralReluctance += result["reluctance"];
            }
            if (result["fringing_factor"] < 1) {
                std::cout << "fringing_factor " << result["fringing_factor"] << std::endl;
            }
            CHECK(result["fringing_factor"] >= 1);
            CHECK(result["maximum_storable_energy"] >= 0);

        }
        calculatedReluctance = coreReluctance + calculatedCentralReluctance + 1 / calculatedLateralReluctance;

        auto error = abs(expectedReluctance - calculatedReluctance) / expectedReluctance;
        std::pair<json, double> aux;
        json jsonAux;
        to_json(jsonAux, gapping[0]);
        aux.first = jsonAux;
        aux.second = error;
        testErrorVersusC1[modelName].push_back(aux);
        if (error > maximumErrorReluctance) {
            std::cout << "error " << error * 100 << " %" << std::endl;

        }
        CHECK_CLOSE(calculatedReluctance, expectedReluctance, expectedReluctance * maximumErrorReluctance);
        return error;
    }

    double run_test_energy(OpenMagnetics::ReluctanceModels modelName, std::string shapeName, json basicGapping, double expectedEnergy, int numberStacks=1) {

        double maximumErrorEnergy = maximumErrorEnergies[modelName];
        auto core = get_core(shapeName, basicGapping, numberStacks);
        auto gapping = core.get_functional_description().get_gapping();
        double calculatedEnergy = 0;
        auto reluctanceModel = OpenMagnetics::ReluctanceModel::factory(modelName);

        for (const auto& gap: gapping) {
            auto result = reluctanceModel->get_gap_reluctance(gap);
            calculatedEnergy += result["maximum_storable_energy"];

            CHECK(result["fringing_factor"] >= 1);
            CHECK(result["maximum_storable_energy"] >= 0);

        }

        auto error = fabs(expectedEnergy - calculatedEnergy) / expectedEnergy;
        if (error > maximumErrorEnergy) {
            std::cout << "error " << error * 100 << " %" << std::endl;
        }
        CHECK_CLOSE(calculatedEnergy, expectedEnergy, expectedEnergy * maximumErrorEnergy);
        return error;
    }

    void test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels modelName) {
        double meanError = 0;
        std::vector<std::map<std::string, double>> tests = {
        {
            {"gapLength", 0.4e-3},
            {"expectedReluctance", 3446071}
        },
        {
            {"gapLength", 0.5e-3},
            {"expectedReluctance", 3233532}
        },
        {
            {"gapLength", 0.7e-3},
            {"expectedReluctance", 5514287}
        },
        {
            {"gapLength", 1.08e-3},
            {"expectedReluctance", 6871406}
        },
        {
            {"gapLength", 1.65e-3},
            {"expectedReluctance", 6982156}
        },
        {
            {"gapLength", 0.305e-3},
            {"expectedReluctance", 1736111}
        },
        {
            {"gapLength", 0.305e-3},
            {"expectedReluctance", 1736111}
        }};

        for (auto& test : tests) {
            meanError += run_test_reluctance(modelName, "PQ 28/20", get_grinded_gap(test["gapLength"]), test["expectedReluctance"]);
        }
        meanError /= tests.size();
        testAverageErrors[modelName].push_back(meanError);
        std::cout << "Mean Error in Reluctance for Test_reluctance_PQ_28_20_Grinded with Model " << magic_enum::enum_name(modelName) << ": " << meanError * 100 << " %" << std::endl;
        std::cout << "Current average for  " << magic_enum::enum_name(modelName) << ": " << std::reduce(testAverageErrors[modelName].begin(), testAverageErrors[modelName].end()) / testAverageErrors[modelName].size() * 100 << " %" << std::endl;
    }

    void test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels modelName) {
        auto constants = OpenMagnetics::Constants();
        double meanError = 0;

        std::vector<std::map<std::string, double>> tests = {
            {
                {"gapLength", constants.residualGap},
                {"expectedReluctance", 187891}
            },
            {
                {"gapLength", 0.0001},
                {"expectedReluctance", 806451}
            },
            {
                {"gapLength", 0.00013},
                {"expectedReluctance", 1035315}
            },
            {
                {"gapLength", 0.00015},
                {"expectedReluctance", 1083841}
            },
            {
                {"gapLength", 0.00017},
                {"expectedReluctance", 1358408}
            },
            {
                {"gapLength", 0.00020},
                {"expectedReluctance", 1513877}
            },
            {
                {"gapLength", 0.0004},
                {"expectedReluctance", 2441604}
            },
            {
                {"gapLength", 0.0005},
                {"expectedReluctance", 3142238}
            },
            {
                {"gapLength", 0.001},
                {"expectedReluctance", 4940440}
            },
        };

        for (auto& test : tests) {
            meanError += run_test_reluctance(modelName, "E 42/21/20", get_spacer_gap(test["gapLength"]), test["expectedReluctance"]);
        }
        meanError /= tests.size();
        testAverageErrors[modelName].push_back(meanError);
        std::cout << "Mean Error in Reluctance for Test_reluctance_E42_21_20_Spacer with Model " << magic_enum::enum_name(modelName) << ": " << meanError * 100 << " %" << std::endl;
        std::cout << "Current average for  " << magic_enum::enum_name(modelName) << ": " << std::reduce(testAverageErrors[modelName].begin(), testAverageErrors[modelName].end()) / testAverageErrors[modelName].size() * 100 << " %" << std::endl;
    
    }

    void test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels modelName) {
        double meanError = 0;
        std::vector<std::map<std::string, double>> tests = {
            {
                {"gapLength", 0.0001},
                {"expectedReluctance", 565899}
            },
            {
                {"gapLength", 0.00013},
                {"expectedReluctance", 698549}
            },
            {
                {"gapLength", 0.00015},
                {"expectedReluctance", 752248}
            },
            {
                {"gapLength", 0.00017},
                {"expectedReluctance", 905486}
            },
            {
                {"gapLength", 0.00020},
                {"expectedReluctance", 1018686}
            },
            {
                {"gapLength", 0.0004},
                {"expectedReluctance", 1610444}
            },
            {
                {"gapLength", 0.0005},
                {"expectedReluctance", 2053962}
            },
            {
                {"gapLength", 0.001},
                {"expectedReluctance", 3247502}
            },

        };

        for (auto& test : tests) {
            meanError += run_test_reluctance(modelName, "ETD 59", get_spacer_gap(test["gapLength"]), test["expectedReluctance"]);
        }
        meanError /= tests.size();
        testAverageErrors[modelName].push_back(meanError);
        std::cout << "Mean Error in Reluctance for Test_reluctance_ETD_59_Spacer with Model " << magic_enum::enum_name(modelName) << ": " << meanError * 100 << " %" << std::endl;    
        std::cout << "Current average for  " << magic_enum::enum_name(modelName) << ": " << std::reduce(testAverageErrors[modelName].begin(), testAverageErrors[modelName].end()) / testAverageErrors[modelName].size() * 100 << " %" << std::endl;
    }

    void test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels modelName) {
        double meanError = 0;
        std::vector<std::map<std::string, double>> tests = {
            {
                {"gapLength", 0.001},
                {"expectedReluctance", 3091787}
            },
            {
                {"gapLength", 0.0015},
                {"expectedReluctance", 4050632}
            },
            {
                {"gapLength", 0.002},
                {"expectedReluctance", 5079365}
            },

        };

        for (auto& test : tests) {
            meanError += run_test_reluctance(modelName, "E 55/28/21", get_spacer_gap(test["gapLength"]), test["expectedReluctance"]);
        }
        meanError /= tests.size();
        testAverageErrors[modelName].push_back(meanError);
        std::cout << "Mean Error in Reluctance for Test_reluctance_E_55_28_21_Spacer with Model " << magic_enum::enum_name(modelName) << ": " << meanError * 100 << " %" << std::endl;
        std::cout << "Current average for  " << magic_enum::enum_name(modelName) << ": " << std::reduce(testAverageErrors[modelName].begin(), testAverageErrors[modelName].end()) / testAverageErrors[modelName].size() * 100 << " %" << std::endl;
    
        // std::cout << "Current error versus C1 for  " << magic_enum::enum_name(modelName) << ": " << std::endl;
        // for (auto & ea: testErrorVersusC1[modelName]) {
        //     std::cout << ea.first << ": " << ea.second << std::endl;
        // }
    }

    void test_energy_pq_40_40_grinded(OpenMagnetics::ReluctanceModels modelName) {
        double meanError = 0;
        std::vector<std::map<std::string, double>> tests = {
            {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            },

        };

        for (auto& test : tests) {
            meanError += run_test_energy(modelName, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }
        meanError /= tests.size();
        std::cout << "Mean Error in Energy for Test_reluctance_PQ_40_40_Grinded with Model " << magic_enum::enum_name(modelName) << ": " << meanError * 100 << " %" << std::endl;
    
    }

    SUITE(ZhangModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::ZHANG);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::ZHANG);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::ZHANG);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::ZHANG);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::ZHANG, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::ZHANG, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::ZHANG, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::ZHANG, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::ZHANG, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }

    SUITE(MuehlethalerModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::MUEHLETHALER);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::MUEHLETHALER);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::MUEHLETHALER);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::MUEHLETHALER);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MUEHLETHALER, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MUEHLETHALER, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MUEHLETHALER, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MUEHLETHALER, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MUEHLETHALER, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }

    SUITE(EffectiveAreaModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_AREA, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }

    SUITE(EffectiveLengthModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::EFFECTIVE_LENGTH, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }

    SUITE(McLymanModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::MCLYMAN);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::MCLYMAN);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::MCLYMAN);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::MCLYMAN);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MCLYMAN, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MCLYMAN, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MCLYMAN, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MCLYMAN, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::MCLYMAN, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }

    SUITE(StengleinModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::STENGLEIN);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::STENGLEIN);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::STENGLEIN);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::STENGLEIN);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::STENGLEIN, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::STENGLEIN, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::STENGLEIN, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::STENGLEIN, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::STENGLEIN, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }

    SUITE(ClassicModel)
    {
        TEST(Test_PQ_28_20_Grinded)
        {
            test_reluctance_pq_28_20_grinded(OpenMagnetics::ReluctanceModels::CLASSIC);
        }

        TEST(Test_E42_21_20_Spacer)
        {
            test_reluctance_e42_21_20_spacer(OpenMagnetics::ReluctanceModels::CLASSIC);
        }

        TEST(Test_ETD_59_Spacer)
        {
            test_reluctance_etd_59_spacer(OpenMagnetics::ReluctanceModels::CLASSIC);
        }

        TEST(Test_E_55_28_21_Spacer)
        {
            test_reluctance_e_55_28_21_spacer(OpenMagnetics::ReluctanceModels::CLASSIC);
        }

        TEST(Test_Energy_PQ_40_40_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.02528}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::CLASSIC, "PQ 40/40", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.09}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::CLASSIC, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"]);
        }

        TEST(Test_Energy_E_80_38_20_2_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.1629}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::CLASSIC, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 2);
        }

        TEST(Test_Energy_E_80_38_20_3_Stacks_Grinded)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.003},
                {"expectedEnergy", 0.24}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::CLASSIC, "E 80/38/20", get_grinded_gap(test["gapLength"]), test["expectedEnergy"], 3);
        }

        TEST(Test_Energy_PQ_40_40_Distributed)
        {
            std::map<std::string, double> test = {
                {"gapLength", 0.002},
                {"expectedEnergy", 0.076}
            };

            run_test_energy(OpenMagnetics::ReluctanceModels::CLASSIC, "PQ 40/40", get_distributed_gap(test["gapLength"], 3), test["expectedEnergy"], 1);
        }
    }
}
