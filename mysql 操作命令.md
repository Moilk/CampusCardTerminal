* 学生信息增加数据： 
> insert into student set
> student_number = '',
> student_name = '',
> student_sex = '';

* 校园卡基本信息表： 
> insert into card_basic set
> card_number = '',
> student_number = '',
> card_time = '',
> card_validity = '',
> equipment_number = '',
> worker_number = '';

* 充值增加数据： 
> insert into card_recharge set
> card_number = '',
> recharge_amount = '',
> recharge_time = '',
> equipment_number = '',
> worker_number = '';

* 消费增加数据： 
> insert into student_purchase set
> card_number = '',
> purchase_amount = '',
> purchase_time = '',
> equipment_number = '';

* 删除充值数据： 
> delete from card_recharge where card_number = '';

* 删除消费数据： 
> delete from student_purchase where card_number = '';

* 删除过期记录： 
> delete from card_basic where datediff(curdate(), card_validity)>=0;

* 显示消费记录某一行的数据：
> select * from student_purchase where card_number = '';

* 将设备信息与消费记录合并后显示：
> select * from student_purchase join equipment on student_purchase.equipment_number = equitment.equipment_number where card_number = '';

* 将消费记录按时间顺序显示： 
> select * from student_purchase where card_number = '' order by purchase_time desc;
