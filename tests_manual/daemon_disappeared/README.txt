Note: This test introduces a new class of tests that are not suitable to
be run automatically, but rather manually due to the destuctive, burn-in
or reboot nature of the tests.


Title: daemon disappeared


Description:
This test attempts to provoke a "*NO* daemon at audit_pid=" message
(other than the well-known harmless audit_pid=0 case).  In some cases
this was caused by an rhashtable bug affecting the lookup of the netlink
socket to the daemon.  In other cases, it was thought to be caused by an
exhaustion of the portid corresponding the the usespace process' PID.

It tries to do this by:
- Set up a repeated "auditctl -l" loop that simply cycles through
  PIDs/portids and exercises the command lock.

- Set up a watch rule on a test directory file and touching the test
  directory file every second to generate an audit event requiring a
  write from the kernel to the daemon socket.

- Restart the daemon every two seconds to try to grab a PID/portid
  combination that don't match so that a write from the kernel queue
  will fail.

- Exit gracefully on ^-C and clean up the child processes.


Run the test with:
cd tests_manual/daemon-disappeared
./test


Todo:
- Improve the portid consumption process to use and occupy more portids,
  in particular portids that don't match a PID in use, forcing a new
  daemon off its default portid maching its PID.


Author: Richard Guy Briggs <rgb@redhat.com> 2017-02-03
