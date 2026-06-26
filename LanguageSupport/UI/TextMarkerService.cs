using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using ICSharpCode.AvalonEdit.Document;
using ICSharpCode.AvalonEdit.Rendering;

namespace NcIdeNative.LanguageSupport.UI
{
    public interface ITextMarker
    {
        int StartOffset { get; }
        int EndOffset { get; }
        int Length { get; }
        void Delete();
        bool IsDeleted { get; }
        Color MarkerColor { get; set; }
        object ToolTip { get; set; }
    }

    public class TextMarkerService : DocumentColorizingTransformer, IBackgroundRenderer, ITextViewConnect
    {
        private readonly TextDocument _document;
        private readonly TextSegmentCollection<TextMarker> _markers;
        private readonly List<TextView> _textViews = new List<TextView>();

        public TextMarkerService(TextDocument document)
        {
            _document = document ?? throw new ArgumentNullException(nameof(document));
            _markers = new TextSegmentCollection<TextMarker>(document);
        }

        public ITextMarker Create(int startOffset, int length)
        {
            int textLength = _document.TextLength;
            if (startOffset < 0 || startOffset > textLength)
                throw new ArgumentOutOfRangeException(nameof(startOffset));
            if (length < 0 || startOffset + length > textLength)
                throw new ArgumentOutOfRangeException(nameof(length));

            var m = new TextMarker(this, startOffset, length);
            _markers.Add(m);
            return m;
        }

        public void Remove(ITextMarker marker)
        {
            if (marker == null)
                throw new ArgumentNullException(nameof(marker));
            var m = marker as TextMarker;
            if (m != null && _markers.Remove(m))
            {
                Redraw(m);
            }
        }

        public void Clear()
        {
            foreach (var m in _markers.ToArray())
            {
                Remove(m);
            }
        }

        internal void RemoveInternal(TextMarker marker)
        {
            if (_markers.Remove(marker))
            {
                Redraw(marker);
            }
        }

        private void Redraw(TextMarker marker)
        {
            foreach (var view in _textViews)
            {
                if (view != null && view.VisualLinesValid)
                {
                    view.Redraw(marker, System.Windows.Threading.DispatcherPriority.Normal);
                }
            }
        }

        public IEnumerable<ITextMarker> TextMarkers => _markers;

        #region IBackgroundRenderer
        public KnownLayer Layer => KnownLayer.Selection;

        public void Draw(TextView textView, DrawingContext drawingContext)
        {
            if (textView == null) throw new ArgumentNullException(nameof(textView));
            if (drawingContext == null) throw new ArgumentNullException(nameof(drawingContext));

            if (_markers == null || !textView.VisualLinesValid) return;

            var visualLines = textView.VisualLines;
            if (visualLines.Count == 0) return;

            int viewStart = visualLines.First().FirstDocumentLine.Offset;
            int viewEnd = visualLines.Last().LastDocumentLine.EndOffset;

            foreach (var marker in _markers.FindOverlappingSegments(viewStart, viewEnd - viewStart))
            {
                if (marker.MarkerColor != Colors.Transparent)
                {
                    foreach (var rect in BackgroundGeometryBuilder.GetRectsForSegment(textView, marker))
                    {
                        Point startPoint = rect.BottomLeft;
                        Point endPoint = rect.BottomRight;

                        // Draw squiggle
                        const double SquiggleSize = 3;
                        Pen squigglePen = new Pen(new SolidColorBrush(marker.MarkerColor), 1);
                        squigglePen.Freeze();

                        StreamGeometry geometry = new StreamGeometry();
                        using (StreamGeometryContext ctx = geometry.Open())
                        {
                            ctx.BeginFigure(startPoint, false, false);
                            double x = startPoint.X;
                            bool up = true;
                            while (x < endPoint.X)
                            {
                                x += SquiggleSize;
                                double y = startPoint.Y + (up ? -SquiggleSize : 0);
                                ctx.LineTo(new Point(x, y), true, false);
                                up = !up;
                            }
                        }
                        geometry.Freeze();
                        drawingContext.DrawGeometry(null, squigglePen, geometry);
                    }
                }
            }
        }
        #endregion

        #region DocumentColorizingTransformer
        protected override void ColorizeLine(DocumentLine line)
        {
        }
        #endregion

        #region ITextViewConnect
        public void AddToTextView(TextView textView)
        {
            if (textView != null && !_textViews.Contains(textView))
            {
                _textViews.Add(textView);
            }
        }

        public void RemoveFromTextView(TextView textView)
        {
            if (textView != null)
            {
                _textViews.Remove(textView);
            }
        }
        #endregion
    }

    public sealed class TextMarker : TextSegment, ITextMarker
    {
        private readonly TextMarkerService _service;

        public TextMarker(TextMarkerService service, int startOffset, int length)
        {
            _service = service ?? throw new ArgumentNullException(nameof(service));
            StartOffset = startOffset;
            Length = length;
        }

        public bool IsDeleted => !IsConnectedToCollection;

        public void Delete()
        {
            _service.RemoveInternal(this);
        }

        public Color MarkerColor { get; set; }
        public object ToolTip { get; set; }
    }
}
