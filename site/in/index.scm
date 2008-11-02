(h3 "DESCRIPTION")
(p "Hybrid is a kernel being developed with an emphasis on design and "
   "portability. It is largely implemented in C/C++, with a small amount of "
   "assembly. The kernel is built on top of "
   (a (@ (href "http://www.nongnu.org/elklib")) "elklib") ".")

(h4 "Features")
(p "The architecture indipendent features are:")
(ul
 (li "Strict architecture-independant layer that allows clean porting "
     "to other architectures")
 (li "Integrated debugger support")
 (li "Symbolic backtraces (via elklib)")
 (li "Multiboot 1.0 compliance (2.0 in progress)")
 (li "Bootmem allocator")
 (li "Memory partitions")
 (li "Paging / Virtual Memory")
 (li "Interrupts / DMA handling")
 (li "Scheduling"))

(h4 "Ports")
(p "The only available port is for the x86 architecture"
   (br)
   (br)
   "The x86 port supports the following features/hardware (maybe not updated):")
(ul
 (li "IDT / GDT")
 (li "PIT (i8253)")
 (li "BIOS")
 (li "CMOS")
 (li "IRQs (i8259)")
 (li "TRAPs")
 (li "DMA (i8237)")
 (li "Debugging via Bochs (e9 port)")
 (li "Debugging via RS232"))

(h3 "COPYING")
(p "The project is licensed under the "
   (a (@ (href "http://www.gnu.org/licenses/licenses.html"))
      "GNU General Public License, version 2"))

(h3 "MAINTAINERS")
(p "Francesco Salvestrini <salvestrini AT gmail DOT com>")

(h3 "AUTHORS")
(p "Francesco Salvestrini <salvestrini AT gmail DOT com>")

(h3 "RELEASES")
(p "Sorry, no public release available at the moment.")

(h3 "MAILING LISTS")
(p "The project has a single moderated mailing list, with an archive. "
   "In order to post a message to the mailing list you must be subscribed. "
   "Please consult the "
   (a (@ (href "http://lists.nongnu.org/mailman/listinfo/hybrid-generic"))
      "mailing list page")
   " for more information on subscribing to the mailing list.")

(h3 "REPORT A BUG")
(p "If you think you have found a bug then please send as complete a report "
   "as possible to "
   "<hybrid-generic AT nongnu DOT org>. "
   "An easy way to collect all the required information, such as platform and "
   "compiler, is to include in your report the config.log file available at "
   "the end of the configuration procedure. "
   (br)
   (br)
   "If you have a patch for a bug that hasn't yet been fixed in the "
   "latest repository sources, please be so kind to create it using the "
   "repository sources, not the release sources.")

(h3 "DEVELOPMENT")
(h4 "Browsing sources")
(p "You can browse the "
   (a (@ (href "http://git.savannah.nongnu.org/gitweb/?p=hybrid.git"))
      "Git repository")
   " of this project with your web browser. This gives you a good "
   "picture of the current status of the source files. You may also view "
   "the complete histories of any file in the repository as well as "
   "differences among two versions.")

(h4 "Getting a copy of the Git Repository")
(p "Anonymous checkout: ")
(pre (@ class "terminal")
     (a (@ (href "http://savannah.gnu.org/maintenance/UsingGit")) "git")
     " clone git://git.savannah.nongnu.org/hybrid.git")

(h4 "Contribute")
(p "If you have time and programming skills, you can help us by developing "
   "missing features, regression tests or bug-fixing the present codebase. "
   "Subscribe to the "
   (a (@ (href "http://lists.nongnu.org/mailman/listinfo/hybrid-generic"))
      "mailing list")
   ", drop us a mail and start coding. Send your code to the "
   "mailing list under the form of patches for the current revision system."
   (br)
   (br)
   "If you have time but no programming skills, you can help with "
   "documentation,  packaging, tests before releases etc ...")
