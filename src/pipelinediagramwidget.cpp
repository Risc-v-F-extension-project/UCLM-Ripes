#include "pipelinediagramwidget.h"
#include "ui_pipelinediagramwidget.h"

#include <QClipboard>
#include <QHeaderView>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QVector>

#include "pipelinediagrammodel.h"
#include "ripessettings.h"

namespace Ripes {

namespace {
const QColor producerBorder("#8D77B8");
const QColor consumerBorder("#63A383");

class ForwardingBorderDelegate : public QStyledItemDelegate {
public:
  explicit ForwardingBorderDelegate(QObject *parent)
      : QStyledItemDelegate(parent) {}

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override {
    QStyledItemDelegate::paint(painter, option, index);
    const auto state = index.data(PipelineDiagramModel::ForwardingRole).toInt();
    if (state == PipelineDiagramModel::NoForwarding)
      return;

    painter->save();
    painter->setPen(QPen(state == PipelineDiagramModel::Consumer
                            ? consumerBorder : producerBorder, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(option.rect.adjusted(1, 1, -1, -1));
    painter->restore();
  }
};
} // namespace

PipelineDiagramWidget::PipelineDiagramWidget(PipelineDiagramModel *model,
                                             QWidget *parent)
    : QDialog(parent), m_ui(new Ui::PipelineDiagramWidget) {
  m_ui->setupUi(this);

  m_stageModel = model;
  m_ui->pipelineDiagramView->setModel(m_stageModel);
  m_ui->pipelineDiagramView->setItemDelegate(
      new ForwardingBorderDelegate(m_ui->pipelineDiagramView));

  auto *legend = new QWidget(this);
  auto *legendLayout = new QHBoxLayout(legend);
  legendLayout->setContentsMargins(0, 0, 0, 0);
  legendLayout->setSpacing(6);
  const auto addLegendEntry = [&](const QColor &color, const QString &text) {
    auto *sample = new QFrame(legend);
    sample->setFixedSize(14, 14);
    sample->setStyleSheet(
        QString("border: 2px solid %1; background: transparent;")
            .arg(color.name()));
    legendLayout->addWidget(sample);
    legendLayout->addWidget(new QLabel(text, legend));
  };
  addLegendEntry(producerBorder, tr("Provides an operand"));
  legendLayout->addSpacing(12);
  addLegendEntry(consumerBorder, tr("Receives an operand"));
  legendLayout->addStretch();
  m_ui->verticalLayout->insertWidget(1, legend);

  m_ui->copy->setIcon(QIcon(":/icons/documents.svg"));
}

PipelineDiagramWidget::~PipelineDiagramWidget() { delete m_ui; }

void PipelineDiagramWidget::prepareForView() {
  const int columns = m_stageModel->columnCount();
  const int rows = m_stageModel->rowCount();
  const auto generation = m_stageModel->historyGeneration();
  if (generation == m_preparedGeneration &&
      columns == m_preparedColumns && rows == m_preparedRows)
    return;

  auto *view = m_ui->pipelineDiagramView;
  const bool reuseWidths = generation == m_preparedGeneration &&
                          rows == m_preparedRows && m_preparedColumns >= 0 &&
                          columns >= m_preparedColumns;
  QVector<int> widths;
  if (reuseWidths) {
    widths.reserve(m_preparedColumns);
    for (int column = 0; column < m_preparedColumns; ++column)
      widths.push_back(view->columnWidth(column));
  }
  const int horizontalPosition = view->horizontalScrollBar()->value();
  const int verticalPosition = view->verticalScrollBar()->value();

  m_stageModel->prepareForView();
  for (int column = 0; column < widths.size(); ++column)
    view->setColumnWidth(column, widths[column]);
  for (int column = widths.size(); column < columns; ++column)
    view->resizeColumnToContents(column);

  m_preparedColumns = columns;
  m_preparedRows = rows;
  m_preparedGeneration = generation;
  view->horizontalScrollBar()->setValue(reuseWidths ? horizontalPosition : 0);
  view->verticalScrollBar()->setValue(reuseWidths ? verticalPosition : 0);
}

void PipelineDiagramWidget::on_copy_clicked() {
  // Copy entire table to clipboard, including headers
  Q_ASSERT(m_stageModel != nullptr);
  QString textualRepr;

  // Copy headers
  textualRepr.append('\t');
  for (int j = 0; j < m_stageModel->columnCount(); j++) {
    textualRepr.append(m_stageModel->headerData(j, Qt::Horizontal).toString());
    textualRepr.append('\t');
  }
  textualRepr.append('\n');
  // Copy data
  for (int i = 0; i < m_stageModel->rowCount(); ++i) {
    textualRepr.append(m_stageModel->headerData(i, Qt::Vertical).toString());
    textualRepr.append('\t');
    for (int j = 0; j < m_stageModel->columnCount(); j++) {
      textualRepr.append(
          m_stageModel->data(m_stageModel->index(i, j)).toString());
      textualRepr.append('\t');
    }
    textualRepr.append('\n');
  }
  QApplication::clipboard()->setText(textualRepr);
}
} // namespace Ripes
