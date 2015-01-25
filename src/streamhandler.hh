#ifndef _XMPP_STREAMHANDLER_HH
#define _XMPP_STREAMHANDLER_HH
#pragma once

#include <memory>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/ContentHandler.hpp>

namespace xmpp {
  
  class Client;

  class StreamHandler : public xercesc::ContentHandler
  {
    public:
      StreamHandler(Client* client);
      ~StreamHandler();

    public:
      void startStream();
      void endStream();

    public:
      void characters(const XMLCh* const chars, const XMLSize_t length);

      void startElement(const XMLCh* const uri, const XMLCh* const localname, 
                        const XMLCh* const qname, const xercesc::Attributes& attributes);
      void endElement(const XMLCh* const uri, const XMLCh* const localname, 
                        const XMLCh* const qname);

      void startDocument();
      void endDocument();

      void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);

      void processingInstruction(const XMLCh* const target, const XMLCh* const data);

      void setDocumentLocator(const xercesc::Locator*);

      void startPrefixMapping(const XMLCh*, const XMLCh*);
      void endPrefixMapping(const XMLCh*);

      void skippedEntity(const XMLCh*);

    private:
      Client* client;
      std::unique_ptr<xercesc::SAX2XMLReader> reader;
  } /* class StreamHandler */;

  class StreamHandlerUtils
  {
    private:
      StreamHandlerUtils();
      ~StreamHandlerUtils();
    
    public:
      static StreamHandlerUtils* get_instance();
  } /* class StreamHandlerUtils */;

} /* ns xmpp */;

#endif /* _XMPP_STREAMHANDLER_HH */