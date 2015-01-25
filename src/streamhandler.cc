#include "streamhandler.hh"

#include <iostream>
#include <string>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "client.hh"

namespace xmpp {
  
  /**
    * STREAMHANDLER
    */
  StreamHandler::StreamHandler(Client* client)
    : client(client)
  {
    StreamHandlerUtils::get_instance(); // init

    this->reader.reset(xercesc::XMLReaderFactory::createXMLReader());
    this->reader->setContentHandler(this);
  }

  StreamHandler::~StreamHandler()
  {

  }

  void StreamHandler::startStream()
  {
    std::stringstream out_stream;
    std::string host_name = this->client->con.get_addr().get_addr();
    out_stream    << "<?xml version='1.0'?>"
                  << "<stream:stream"
                  <<    " from='terra-xmpp-test@" << host_name << "'"
                  <<    " to='" << host_name << "'"
                  <<    " version='1.0'"
                  <<    " xml:lang='en'"
                  <<    " xmlns='jabber:client'"
                  <<    " xmlns:stream='http://etherx.jabber.org/streams'>"
                  << "\r\n" << "\r\n";

    this->client->con.send(out_stream);
  }

  void StreamHandler::endStream()
  {
    std::stringstream out_stream;
    out_stream    << "</stream:stream>"
                  << "\r\n" << "\r\n";

    this->client->con.send(out_stream);

    if(0 < this->client->con.recv())
    {
      std::string rdata = ((std::stringbuf *)this->client->con.get_input_stream().rdbuf())->str();

      this->reader->parse(
        xercesc::MemBufInputSource((XMLByte *) rdata.c_str(), rdata.length(), 
        "foo"));
    }
  }

  /**
    * STREAMHANDLER -> XERCESC::CONTENTHANDLER
    */
  void StreamHandler::characters(const XMLCh* const chars, const XMLSize_t length)
  {
    for(auto i=0; i<length; i++) 
        std::wcout << (wchar_t)chars[i];
      std::wcout << std::endl;
  }

  void StreamHandler::startElement(const XMLCh* const uri, const XMLCh* const localname, 
                    const XMLCh* const qname, const xercesc::Attributes& attributes)
  {
    std::wcout << "<" << (wchar_t*)localname << ">" << std:: endl;
  }

  void StreamHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, 
                    const XMLCh* const qname)
  {
    std::wcout << "</" << (wchar_t*)localname << ">" << std:: endl;
  }

  void StreamHandler::startDocument()
  {

  }

  void StreamHandler::endDocument()
  {

  }

  void StreamHandler::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length)
  {

  }

  void StreamHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data)
  {

  }

  void StreamHandler::setDocumentLocator(const xercesc::Locator*)
  {

  }

  void StreamHandler::startPrefixMapping(const XMLCh*, const XMLCh*)
  {

  }

  void StreamHandler::endPrefixMapping(const XMLCh*)
  {

  }

  void StreamHandler::skippedEntity(const XMLCh*)
  {

  }

  /**
    * STREAMHANDLERUTILS
    */
  StreamHandlerUtils::StreamHandlerUtils()
  {
    xercesc::XMLPlatformUtils::Initialize();
  }

  StreamHandlerUtils::~StreamHandlerUtils()
  {
    xercesc::XMLPlatformUtils::Terminate();
  }

  StreamHandlerUtils* StreamHandlerUtils::get_instance()
  {
    static StreamHandlerUtils shu;
    return &shu;
  }

} /* ns xmpp */;
