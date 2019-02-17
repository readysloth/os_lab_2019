gcc utils.c find_min_max.c parallel_min_max.c -o minMaxPR_LL.exe
./minMaxPR_LL.exe --seed 98734 --array 20 --pnum 4
echo =============================================
./minMaxPR_LL.exe --seed 1234 --array 20 --pnum 4 --by_files