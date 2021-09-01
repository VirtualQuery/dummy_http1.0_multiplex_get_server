Just a server that used to pass some automated grader.
Currently uses select multiplexing.
poll/epoll and event-based approaches might be tested later.
Has dummy threadpool with job queue of void(*)s.
Returns dummy answers to some basic GET requests.
Starts on Linux.
