#ifndef TCP_EASY_H
# define TCP_EASY_H

#define sin6p(sap) ((struct sockaddr_in6 *)(sap))

/**
 * @file   tcp_easy_ipv46dns.h
 * @author Cameron Kerr <ckerr@@cs.otago.ac.nz>
 * @date   Wed Apr  6 08:34:00 2011
 * 
 * @brief  Easily create TCP active and passive sockets.
 *
 * These make using the Sockets API much easier, and abstracts away
 * much of the boiler-plate code involved in a TCP client or server,
 * although for servers it will only be useful (currently) for simple
 * servers -- ones that listen on multiple interfaces will want to
 * create a wrapper around tcp_listen that is used here.
 *
 * @section design Design
 *
 * The design of a basic TCP connect or listen is fairly
 * straight-forward, but is cumbersome because much of the code is
 * fairly boilerplate, which could easily be abstracted into a
 * higher-layer API.
 *
 * Because we are aiming for convenience, we don't want the API user
 * to need to do any address parsing or DNS lookups
 * themselves. Instead, the API is based largely around strings, as
 * that is generally what the caller has available from command-line
 * arguments or configuration files.
 *
 * By passing in strings, we also give ourselves greater flexibility,
 * such as IPv4 and IPv6 addresses or DNS names.
 *
 * @section error_handling Error Handling
 * 
 * This library could fail for numerous reasons, with the two most likely
 * being related to address-parsing and name-lookups (both relating to
 * the underlying getaddrinfo() or getnameinfo()), or due to a socket-level
 * error (such as connect() or bind() failing.
 *
 * To make it easy to communicate the reason for the failure, a string
 * explaining the error can be determined using tcp_strerror(), which
 * combines gai_strerror() and strerror().
 *
 * @section alternative_design Alternative [Unimplemented] Design
 *
 * There are a number of ways in which we might want to change the way
 * we use tcp_connect() and tcp_listen(); for example, when connecting
 * we may want to specify a timeout, or specify that we only want to
 * use IPv6 only and not fall back to IPv4. To cater for this (future)
 * feature set, there are a few options:
 *
 * @li Either have a structure into which we put all our preferences, and
 *     then pass that structure to an enhanced version of tcp_connect()
 *     or tcp_listen().
 *
 * @li Take an object-oriented approach, and use setter-methods to set up
 *     our preferences, before using another method to initiate the actual
 *     connection or listen.
 *
 * @li Or we could use variadic functions (meaning functions that take
 *     a variable number of arguments (arity), such as printf).
 *
 * Because this API is meant to be very convenient, we might choose the
 * last method, which is must less bulky to use. It is useful to point out
 * that this is the style of the GTK+ API for GUI programming.
 *
 * Here is an example of what such a function @em might be declared:
 *
 * @code
 * int tcp_listen_keyval(const char *host, const char *service, ...);
 * @endcode
 *
 * We might use such a function by passing keyword-argument pairs, and
 * passing a final keyword-argument to signify the end of the
 * arguments. Here is a possible example of how such a function
 * @em might be called:
 *
 * @code
 * // Remember, this is not implemented!
 * sock = tcp_listen_keyval("0.0.0.0", "8421",
 *                          tcp_kv_reuse_address, 1,
 *                          tcp_kv_listen_queue, 1,
 *                          tcp_kv_allow_ipv6, 0,
 *                          tcp_kv_allow_ipv4, 1,
 *                          tcp_kv_use_libwrap_service, "outboard",
 *                          tcp_kv_end);
 * @endcode
 *
 * As you can see, this style of interface, though quite unusual for a
 * C program in its use of keyword-value argument pairs, can easily
 * allow an expressive amount of flexibility.
 *
 * The more typical way however, would be the first option above: to
 * create a structure of some sort, zero it out and set any values you
 * want in there, then pass that structure in to a function like
 * tcp_listen which takes such a structure as its arguments. It's
 * faster (you don't have to process key/value pairs at runtime) but
 * it can be a little more awkward (well, more lines of code) to use.
 *
 * We @em could even go further and start implementing what you might
 * call "behaviours" (from a language called Erlang), whereby you
 * create a highly convenient function that takes care of all the
 * connection handling and reading of data, and just calls call-back
 * functions, that you provide, when it finds a message (determined
 * using another call-back function). You sacrifice some flexibility,
 * but you get dividends in development time (ie. re-useing a library
 * to implement a common pattern) and correctness (ie. re-useing a
 * library that is already tested).
 *
 * I should remind you that this section has not been implemented in
 * this code, and will not be used in Outboard.
 */

/** 
 * @brief String explanation of errors from tcp_connect() and tcp_listen()
 * 
 * @param errval The value returned by tcp_connect() or tcp_listen()
 * 
 * @return A read-only string. Should not be be passed to free().
 */

extern const char *
tcp_strerror(
    int errval);

/** 
 * @brief Make a TCP connection to a host by name or address.
 * 
 * @attention On systems that define AI_ADDRCONFIG (Linux), IPv6 addresses
 * will @em only be returned when at least one interface has a non
 * link-local [or loopback] IPv6 address. This helps to prevent unintended
 * use of IPv6.
 *
 * @param[in] host Either IPv4/6 address or DNS name.
 * @param[in] service Service name or string representation of port number.
 * 
 * @c service is a service name as specified in /etc/services
 * (or equivalent) and returned by getservbyname().
 *
 * @return Connected socket, -1 if all possibilities (eg. DNS name
 *         resolves to multiple addresses) were exhausted, or <0 to
 *         signify some other error (in which tcp_strerror() should
 *         be consulted.
 */

extern int
tcp_easy_connect(                        
    const char *host,
    const char *service);

/** 
 * @brief Easily create a TCP listening socket.
 * 
 * @param[in] interface IPv4/6 interface to bind to.
 * @param[in] service Service name or string representation of port number.
 * @param listen_queue Argument to listen()
 * 
 * @c interface can be either a IPv4 address ("0.0.0.0" for wildcard),
 * IPv6 address ("::" for wildcard), or a DNS name (although this is not
 * recommended).
 *
 * @note The term 'interface' comes from the IPv4 notion of an interface,
 * and does not (at this time) support a network device name (eg. 'eth0')
 *
 * @c service is a service name as specified in /etc/services
 * (or equivalent) and returned by getservbyname().
 *
 * A good default value for @c listen_queue would be 10, although higher
 * for high-performance systems (in which case you would already have a
 * good idea of what this argument does).
 *
 * @attention Dual-stack systems generally have IPv4-mapped IPv6
 * enabled by default, but we turn that off here. This means that if
 * you don't have anything bound to the IPv4 address on port X, then a
 * IPv6 bind on :: on port X will @em only receive IPv6 connections,
 * and not connections with addresses such as ::ffff:A.B.C.D.
 *
 * The rationale for turning off the dual-stack feature is that if you
 * bind to ::, then to 0.0.0.0 on the same port, you get Address
 * already in use. Note that some systems default to turning
 * IPv4-mapped IPv6 addresses (such as Redhat), while others (such as
 * Debian) leave it at the Linux kernel defaults.
 *
 * @note On systems that define AI_ADDRCONFIG (Linux), IPv6 addresses
 * will @em only be used when at least one interface has a non
 * link-local [or loopback] IPv6 address. This helps to prevent
 * unintended use of IPv6. This means that a bind to "::" could fail
 *
 * @return Listening socket or <0 on error, in which tcp_strerror() should
 *         be used.
 */

extern int
tcp_easy_listen(
    const char *interface,
    const char *service,
    int listen_queue);

#endif /* TCP_EASY_H */
