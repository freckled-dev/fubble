#include "add_windows_root_certs.hpp"
#include <wincrypt.h>

void http::add_windows_root_certs(boost::asio::ssl::context &ctx) {
    HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
    if (hStore == nullptr) return;

    X509_STORE *store = X509_STORE_new();
    PCCERT_CONTEXT pContext = nullptr;
    while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != nullptr) {
        X509 *x509 = d2i_X509(nullptr,
                (const unsigned char **)&pContext->pbCertEncoded,
                pContext->cbCertEncoded);
        if(x509 == nullptr) continue;
        X509_STORE_add_cert(store, x509);
        X509_free(x509);
    }

    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    SSL_CTX_set_cert_store(ctx.native_handle(), store);
}