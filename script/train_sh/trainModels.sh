#12c
/home/xileli/Documents/program/CascadeCNN/train_sh/train12c.sh >& log/train12c.log
cat train12c.log | grep "Train net output" | awk '{print $11}' >& log/loss12c.log
cat train12c.log | grep "Test net output #0" | awk '{print $11}' >& log/accuracy12c_test.log
cat train12c.log | grep "Test net output #1" | awk '{print $11}' >& log/loss12c_test.log

#12cal
/home/xileli/Documents/program/CascadeCNN/train_sh/train12cal.sh >& log/train12cal.log
cat train12cal.log | grep "Train net output" | awk '{print $11}' >& log/loss12cal.log
cat train12cal.log | grep "Test net output #0" | awk '{print $11}' >& log/accuracy12cal_test.log
cat train12cal.log | grep "Test net output #1" | awk '{print $11}' >& log/loss12cal_test.log

#24c
/home/xileli/Documents/program/CascadeCNN/train_sh/train24c.sh >& log/train24c.log
cat train24c.log | grep "Train net output" | awk '{print $11}' >& log/loss24c.log
cat train24c.log | grep "Test net output #0" | awk '{print $11}' >& log/accuracy24c_test.log
cat train24c.log | grep "Test net output #1" | awk '{print $11}' >& log/loss24c_test.log

#24cal
/home/xileli/Documents/program/CascadeCNN/train_sh/train24cal.sh >& log/train24cal.log
cat train24cal.log | grep "Train net output" | awk '{print $11}' >& log/loss24cal.log
cat train24cal.log | grep "Test net output #0" | awk '{print $11}' >& log/accuracy24cal_test.log
cat train24cal.log | grep "Test net output #1" | awk '{print $11}' >& log/loss24cal_test.log

#48c
/home/xileli/Documents/program/CascadeCNN/train_sh/train48c.sh >& log/train48c.log
cat train48c.log | grep "Train net output" | awk '{print $11}' >& log/loss48c.log
cat train48c.log | grep "Test net output #0" | awk '{print $11}' >& log/accuracy48c_test.log
cat train48c.log | grep "Test net output #1" | awk '{print $11}' >& log/loss48c_test.log

#48cal
/home/xileli/Documents/program/CascadeCNN/train_sh/train48cal.sh >& log/train48cal.log
cat train48cal.log | grep "Train net output" | awk '{print $11}' >& log/loss48cal.log
cat train48cal.log | grep "Test net output #0" | awk '{print $11}' >& log/accuracy48cal_test.log
cat train48cal.log | grep "Test net output #1" | awk '{print $11}' >& log/loss48cal_test.log
