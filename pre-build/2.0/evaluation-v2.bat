::Extraction 05.04

::Extracts surgical actions using flow-based signatures
start "Extraction" /wait /b xtraction.exe data_mtap-v1_2 1 I:\SQUASH\datasets\data_mtap-v1_2\segments\merged I:\SQUASH\datasets\data_mtap-v1_2\evaluation\segments\merged 2 8000 0 40 C:\Users\skletz\Dropbox\Development\SQUASH\defuse\data\samplepoints 5 2 0.01 0 5 0

::Extracts surgical actions using tracking-based signatures
::start "Extraction" /wait /b xtraction.exe data_mtap-v1_2 1 I:\SQUASH\datasets\data_mtap-v1_2\segments\merged I:\SQUASH\datasets\data_mtap-v1_2\evaluation\segments\merged 1 40000 0 40 C:\Users\skletz\Dropbox\Development\SQUASH\defuse\data\samplepoints 5 2 0.01 0 5 0

::Evaluation 05.04
::start "Extraction" /wait /b valuation.exe data_mtap-v1_2 I:\SQUASH\datasets\data_mtap-v1_2\evaluation\segments\merged\DFS1\DFS1_SFS1_40000_random_40_5_2_0.01_0_0_5 I:\SQUASH\datasets\data_mtap-v1_2\maps\segments\merged 1 1 0 I:\SQUASH\datasets\data_mtap-v1_2\evaluation\data_mtap-v1_2_eval-segments_merged.csv 2 0 1 0 1.0 0
