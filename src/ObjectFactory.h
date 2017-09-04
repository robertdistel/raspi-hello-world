/*
 * ObjectFactory.h
 *
 *  Created on: 4 Sep. 2017
 *      Author: robert
 */

#ifndef OBJECTFACTORY_H_
#define OBJECTFACTORY_H_

#include <string>
#include <memory>
#include <regex>
#include <map>


class ObjectFactory
{
public:

  template<class T>
    class Item
    {
    public:
      Item (std::string name_) :
	  name (name_)
      {
      }

      std::string name;
    };

  ObjectFactory ()
  {
  }


  template<class T>
    std::shared_ptr<T>
    make (std::string parameters)
    {
      T* p = static_cast<T*> (make (parameters));
      return std::shared_ptr<T> (p);
    }

  template<class T>
    ObjectFactory&
    operator() (ObjectFactory::Item<T> item)
    {
      std::function<void*
      (std::string)> f ([](std::string parameters)
	{
	  void* p(new T(parameters));
	  return p;
	});

      return operator() (item.name, f);
    }

private:
  void*
  make (std::string parameters)
  {
    std::smatch sm;
    std::regex rex ("([^:]*):(.*)");
    std::regex_match (parameters, sm, rex);
    std::string type = std::string (sm[1]);
    if (constructors.count (type) == 0)
      return nullptr;
    return (constructors[type]) (std::string (sm[2]));
  }

  ObjectFactory&
  operator() (std::string name, std::function<void*
  (std::string)> constructor)
  {
    constructors[name] = constructor;
    return *this;
  }

  std::map<std::string, std::function<void*
  (std::string)> > constructors;
};
/* an example of registering classes in the factor
ObjectFactory object_factory =    ObjectFactory () (ObjectFactory::Item<Stdio> ("stdio"))
	(ObjectFactory::Item<CryptoWrapper<Stdio, Encryptor> > ("encrypt-stdio"))
	(ObjectFactory::Item<CryptoWrapper<Stdio, Decryptor> > ("decrypt-stdio"))
	(ObjectFactory::Item<File> ("file"))
	(ObjectFactory::Item<CryptoWrapper<File, Encryptor> > ("encrypt-file"))
	(ObjectFactory::Item<CryptoWrapper<File, Decryptor> > ("decrypt-file"))
	(ObjectFactory::Item<UdpSocket> ("udp"))
	(ObjectFactory::Item<CryptoWrapper<UdpSocket, Encryptor> > ("encrypt-udp"))
	(ObjectFactory::Item<CryptoWrapper<UdpSocket, Decryptor> > ("decrypt-udp"))
	(ObjectFactory::Item<Alsa> ("alsa"))
	(ObjectFactory::Item<CryptoWrapper<Alsa, Encryptor> > ("encrypt-alsa"))
	(ObjectFactory::Item<CryptoWrapper<Alsa, Decryptor> > ("decrypt-alsa"));

	As long as the constructor for the class registered can handle a std::string argument to construct itself it can be built by the factory
	the factory returns nullptr if it cant build the requested object

*/

#endif /* OBJECTFACTORY_H_ */
