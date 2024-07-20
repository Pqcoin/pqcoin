import os
import time
import datetime
n =2016
current_time = datetime.datetime.now()

# 打印当前时间
print("Current time:", current_time)
while(n>=0):
#os.system('pqucoin-cli sendtoaddress DURqHAqv77AoiqXxvLZmV4BtWbgSDpmn4G 1')
    os.system('pqcoin-cli generatetoaddress 1 DMA7q3UP1yT9ZkGRe3UHAeLu9kfDGf6mhs')
    current_time = datetime.datetime.now()

# 打印当前时间
    print("Current time:", current_time)
    time.sleep(0.1)

    n=n-1



# 获取当前时间
current_time = datetime.datetime.now()

# 打印当前时间
print("Current time:", current_time)

#time.sleep(1)
#
#os.system('pqucoin-cli getmempoolinfo')
#DURqHAqv77AoiqXxvLZmV4BtWbgSDpmn4G
