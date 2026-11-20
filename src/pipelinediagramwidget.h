#pragma once

#include <QDialog>

QT_FORWARD_DECLARE_CLASS(QAbstractItemModel)

namespace Ripes {
class PipelineDiagramModel;
namespace Ui {
class PipelineDiagramWidget;
}

class PipelineDiagramWidget : public QDialog {
  Q_OBJECT

public:
  PipelineDiagramWidget(PipelineDiagramModel *model, QWidget *parent = nullptr);
  ~PipelineDiagramWidget() override;
  void prepareForView();

private slots:
  void on_copy_clicked();

private:
  Ui::PipelineDiagramWidget *m_ui = nullptr;
  PipelineDiagramModel *m_stageModel = nullptr;
  int m_preparedColumns = -1;
  int m_preparedRows = -1;
  unsigned long long m_preparedGeneration = 0;
};
} // namespace Ripes
