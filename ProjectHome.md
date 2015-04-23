### Welcome to eXaro's google page ###

**eXaro** it's a **free** report engine similar with Jasper or Crystal Reports. **eXaro** can be embedded **very easy in any Qt application**.

**eXaro** has an integrated preview with **search and export (in different formats) capabilities**


---

If you want to be a contributor please read the [CodingStyle](CodingStyle.md)

---

Starting 02-02-2009 eXaro is relicensed under **GPL** and **LGPL** v3. In other words you can choose to use eXaro under **GPL** or **LGPL** v3.

This project is accepting code only if is released under GPL only or GPL+LGPL license.

I'll not accept code released under LGPL only or other license.

**WARNING** If you choose to use eXaro under **LGPL** license you have to edit top "CMakeLists.txt" file, and choose "LGPL" as eXaro's license, GPL license is used by default. When you choose **LGPL**, eXaro will not compile barcode plugin because this plugin is based on zint library http://www.zint.org.uk which is GPL.


---


### Starting with 1.60 version eXaro need some libs to be fully functional: ###
  * **propertyEditor** http://code.google.com/p/propertyeditor/downloads/list is used to build the report designer.
  * **Zint** http://www.zint.org.uk/ (from CVS) http://sourceforge.net/projects/zint/ **or you can download a "stable" snapshot from http://code.google.com/p/exaro/downloads/list** is used to build barcode plugin **(remember this plugin is build only if you choose GPL license)**
  * **quazip** http://sourceforge.net/projects/quazip/ **or you can download a "cmaked" snapshot from http://code.google.com/p/exaro/downloads/list** is used to build ods export plugin.

