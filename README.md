# pemcrack
Cracks SSL PEM files that hold encrypted private keys. Brute forces or dictionary cracks. This code is extraordinarily slow, DON'T JUDGE ME!!!

##Building

I include some binaries in the `bin` directory. You can trust me, I ran AV
on them and everything.

I'm not spending a lot of time making it easy for you to compile this. Don't
judge me, it's just a straightforward "compile the code and link to the library".

On Windows, you need to install the include files. It's just easiest to build
openssl from scratch. After that, use the Visual Studio project in the `vs10` directory.

On Mac OS X, just compile the following. You'll need to install the compiler,
which comes with XCode developer tools.

  gcc pemcrack.c -o pemcrack -lssl -lcrypto

On Linux, compile the following. You'll need get the `libssl-dev` package. I'm 
not sure the exact name, I'll get around to testing it.

  gcc pemcrack.c -o pemcrack -lssl -lcrypto
  
##See also

Here is another project that does the same thing, in case this projeck doesn't 
satisfy your needs

  ``https://github.com/chipx0r/crackpkcs8``
  
