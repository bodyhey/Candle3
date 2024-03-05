#include "formtoconfigurationtransformer.h"
#include "ui_frmsettings.h"

FormToConfigurationTransformer::FormToConfigurationTransformer() {

}

Configuration* FormToConfigurationTransformer::transformToConfiguration(frmSettings *form)
{
    Configuration *configuration = new Configuration();

    Ui_frmSettings *ui = form->ui;

    configuration->setLanguage(ui->cboLanguage->currentText());

    return configuration;
}
