�����ԣ�Funny Language��
===================================

�����ԣ��ֳƷ����ԣ���һ�ָ�ʽ���ɣ�free style���ĳ�ʽ���ԡ�����ġ���ʽ���ɡ�������Ե����ɣ������Ǿ������ɡ���ʽ������ζ�ſ����Զ��庯�����﷨��ʽ�������﷨��ʽ��������������˵��һ����Ȼ��  
    
�����Բ���ֱ�ӽ�����ת��Ϊ�������ԣ����ǽ�Lisp������Ϊ���м����ԡ���ô���������ǡ��仯�����ԡ���Ҫ��һ�֡���������ԡ���Ϊ���������������Բ���Ӧ��䡣  
    
�����Բ��õ���һ�ִ�������DFA������Ϊʲô��DFA��������NFA��������ΪDFAû�ж����ԣ��������ڴ�����NFA��Ҫת��ΪDFA������ת���ڴ�������������Եø������ѣ���ȫû�б�Ҫ�������ڶ����﷨ģ���ʱ�򣬱���ԭ������DFA�ġ�  
    
�����Ƿ�������RHEL�����еļ���ʾ����  

	[root@192 funny]# ./funny
	Welcome to FUNNY programming world. Type {exit} to exit.
	> {define function {qsum}:
	      {the sum of squares {a} and {b}} as
	        {{{a}*{a}}+{{b}*{b}}}}
	qsum
	> {the sum of squares {2} and {3}}
	13
	> {define function {abs}: 
	      {the absolute value of {x}} as 
	        {for the conditions: 
	           when {{x}>{0}} then {x},
	           when {{x}={0}} then {0},
	           when {{x}<{0}} then {-{x}}}}
	abs
	> {the absolute value of {-2}}
	2
	> {the sum of squares {2} and {the absolute value of {-2}}}
	8
	> {the absolute value of {the sum of squares {2} and {-3}}}
	13
	> {define function {copy-files}:
	  {copy files {n} times from {src} to {dest}} as
	    {if {{n}>{0}} then {begin procedure 
	       {run command {copy file from {src} to {append string {dest} {convert atom {n} to string}}}} 
	       {copy files {{n}-{1}} times from {src} to {dest}}}}}
	copy-files
	> {copy files {4} times from {"/home/fgp/temp/abc"} to {"/home/fgp/temp/def"}}
	()
	> {exit}
	Goodbye!
	[root@192 funny]#

ϣ�������ϲ�����������ԣ�����������չ��  

��ϵ��ʽ
===================================
    qqȺ��111086164  
    ���䣺fanguangping@163.com  

