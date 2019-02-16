SHELL=/bin/bash
RED='\033[1;31m'
YELLOW='\033[1;33m'
BLUE='\033[1;34m'
NC='\033[0m' # No Color
################################
SUM=0
COUNT=0
for number in "$@"" ; do
SUM+=number
COUNT++
done
echo -e "${RED}Количество:${NC}$(COUNT)"
echo -e "${YELLOW}Среднее:${NC}$(SUM/COUNT)"