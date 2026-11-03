"""Embed linked repository sources for offline API documentation."""
from pathlib import Path
from html.parser import HTMLParser
from urllib.parse import unquote, urlsplit
import json
import re

ROOT = Path(__file__).resolve().parents[1]
BEGIN = '<!-- API-SOURCE-VIEWER:BEGIN -->'
END = '<!-- API-SOURCE-VIEWER:END -->'


class SourceLinks(HTMLParser):
    def __init__(self):
        super().__init__()
        self.links = set()

    def handle_starttag(self, tag, attrs):
        if tag == 'a':
            href = dict(attrs).get('href')
            if href:
                self.links.add(href)


def attach(page, content):
    content = re.sub(re.escape(BEGIN) + r'[\s\S]*?' + re.escape(END), '', content)
    parser = SourceLinks()
    parser.feed(content)
    sources = {}
    for href in sorted(parser.links):
        url = urlsplit(href)
        if url.scheme or url.netloc or url.query:
            continue
        relative = unquote(url.path)
        path = (page.parent / relative).resolve()
        if not path.is_relative_to(ROOT) or path.suffix.lower() not in ('.cpp', '.h', '.md'):
            continue
        if path.is_file():
            sources[relative] = dict(name=path.relative_to(ROOT).as_posix(), text=path.read_text(encoding='utf-8-sig'))
    # JSON is data, including when source contains HTML or a closing script tag.
    payload = json.dumps(sources, ensure_ascii=False).replace('&', '\\u0026').replace('<', '\\u003c').replace('>', '\\u003e')
    block = BEGIN + STYLE + DIALOG + '<script type="application/json" id="api-source-data">' + payload + '</script>' + SCRIPT + END
    return content.replace('</body>', block + '</body>')


STYLE = '''<style>
#api-source-dialog{width:min(1100px,94vw);max-width:94vw;max-height:90vh;padding:0;border:1px solid #607b9b;border-radius:10px;background:#f6f8fb;color:#17283c}
#api-source-dialog::backdrop{background:#142238aa}
#api-source-dialog header{display:flex;align-items:center;gap:1rem;flex-wrap:wrap;padding:1rem;border-bottom:1px solid #bdcbdc}
#api-source-title{flex:1;margin:0;font-size:1rem;overflow-wrap:anywhere}
#api-source-lines{overflow:auto;max-height:65vh;overscroll-behavior:contain;background:#142132;color:#e7edf5;margin:0;padding:1rem;font-size:14px;line-height:1.6;tab-size:4;white-space:pre}
#api-source-lines .source-line{display:block;min-height:1.6em}
#api-source-lines .source-line::before{content:attr(data-line);display:inline-block;width:5ch;margin-right:2ch;color:#96acc5;text-align:right;user-select:none}
#api-source-lines .source-selected{background:#624c16;color:#fff4ce}
#api-source-status{padding:0 1rem;overflow-wrap:anywhere}
.api-block-map{width:100%;border-collapse:collapse}.api-block-map td,.api-block-map th{padding:.6rem;border:1px solid #bdcbdc;text-align:left;vertical-align:top}
</style>'''

DIALOG = '''<dialog id="api-source-dialog" aria-labelledby="api-source-title">
<header><h2 id="api-source-title">소스 코드</h2><a id="api-source-original">원본 파일</a><button type="button" id="api-source-close">닫기</button></header>
<p id="api-source-status" role="status"></p><pre id="api-source-lines" tabindex="0"><code></code></pre>
</dialog>'''

SCRIPT = '''<script>
(() => {
  const sources = JSON.parse(document.getElementById('api-source-data').textContent);
  const dialog = document.getElementById('api-source-dialog');
  const code = document.querySelector('#api-source-lines code');
  const original = document.getElementById('api-source-original');
  const close = document.getElementById('api-source-close');
  let opener;
  close.addEventListener('click', () => dialog.close());
  dialog.addEventListener('close', () => { if (opener) opener.focus(); });
  document.addEventListener('click', event => {
    const link = event.target.closest('a');
    if (!link || link === original || event.defaultPrevented || event.button !== 0 || event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) return;
    const href = link.getAttribute('href');
    if (!href) return;
    const url = new URL(href, document.baseURI);
    const page = new URL(document.baseURI);
    if (url.protocol !== page.protocol || url.host !== page.host || !/\\.(cpp|h|md)$/i.test(url.pathname)) return;
    let relative;
    try { relative = decodeURIComponent(href.split(/[?#]/)[0]); } catch { return; }
    if (!Object.prototype.hasOwnProperty.call(sources, relative)) return;
    if (typeof dialog.showModal !== 'function') return;
    event.preventDefault();
    opener = link;
    const source = sources[relative];
    // Existing API metadata places :L123 immediately after the source link.
    const following = link.nextSibling && link.nextSibling.nodeType === Node.TEXT_NODE ? link.nextSibling.textContent : '';
    const range = url.hash.match(/^#L(\\d+)(?:-L?(\\d+))?$/) || following.match(/^:L(\\d+)(?:-L?(\\d+))?/);
    const first = range ? Number(range[1]) : 0;
    const last = range ? Number(range[2] || range[1]) : 0;
    document.getElementById('api-source-title').textContent = source.name;
    original.setAttribute('href', href);
    document.getElementById('api-source-status').textContent = '문서 생성 시점의 소스' + (first ? ' · L' + first + '–L' + last : '') + ' · Esc 또는 닫기로 복귀';
    const fragment = document.createDocumentFragment();
    source.text.replace(/\\r\\n/g, '\\n').split('\\n').forEach((text, index) => {
      const line = document.createElement('span');
      line.className = 'source-line' + (index + 1 >= first && index + 1 <= last ? ' source-selected' : '');
      line.dataset.line = index + 1;
      line.textContent = text || ' ';
      fragment.appendChild(line);
    });
    code.replaceChildren(fragment);
    if (!dialog.open) dialog.showModal();
    const selected = code.querySelector('.source-selected');
    if (selected) selected.scrollIntoView({block:'center'});
    else document.getElementById('api-source-lines').scrollTop = 0;
    close.focus();
  });
})();
</script>'''
