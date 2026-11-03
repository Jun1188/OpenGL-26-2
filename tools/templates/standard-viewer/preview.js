// Markdown 미리보기. documents.js에 묶인 원문을 marked로 그리고, 저장소 안 링크를 미리보기 링크로 바꾼다.
const root = new URL('./', location.href);
const docs = window.STANDARD_DOCUMENTS;
const catalog = window.STANDARD_CATALOG;
const path = new URLSearchParams(location.search).get('doc') || 'README.md';
const record = docs.find(d => d.path === path && d.markdown !== null);
const content = document.querySelector('#content');
const toc = document.querySelector('#toc');

if (!record) {
  content.textContent = '문서를 찾을 수 없습니다. 전체 목차에서 선택하세요.';
} else {
  document.title = record.title + ' · standard 문서';
  document.querySelector('#path').textContent = path;
  document.querySelector('#original').href = new URL(path, root).href;
  document.querySelector('#source').textContent = record.markdown;

  const category = catalog.categories.find(c => c.documents.some(entry => entry.path === path));
  const categoryLink = document.querySelector('#category');
  if (category) {
    categoryLink.textContent = category.title;
    categoryLink.href = 'index.html#cat-' + category.id;
  } else categoryLink.remove();

  const escape = s => s.replace(/[&<>"']/g, c => ({'&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;'}[c]));
  const renderer = new marked.Renderer();
  renderer.html = token => escape(token.text);
  content.innerHTML = marked.parse(record.markdown, {gfm: true, renderer});

  for (const a of content.querySelectorAll('a[href]')) {
    const raw = a.getAttribute('href');
    if (raw.startsWith('#')) continue;
    const url = new URL(raw, new URL(path, root));
    if (!['file:', 'http:', 'https:', 'mailto:'].includes(url.protocol)) { a.removeAttribute('href'); continue; }
    const relative = decodeURIComponent(url.pathname.slice(root.pathname.length));
    const md = docs.find(d => d.path === relative && d.markdown !== null);
    if (url.origin === root.origin && url.pathname.startsWith(root.pathname) && md)
      a.href = new URL('document.html?doc=' + encodeURIComponent(relative) + url.hash, root).href;
    else a.href = url.href;
  }
  for (const img of content.querySelectorAll('img[src]')) {
    const url = new URL(img.getAttribute('src'), new URL(path, root));
    if (['file:', 'http:', 'https:'].includes(url.protocol)) img.src = url.href;
    else img.removeAttribute('src');
  }

  let count = 0;
  const headings = content.querySelectorAll('h1,h2,h3');
  if (headings.length > 1) {
    const title = document.createElement('div');
    title.className = 'toc-title';
    title.textContent = '이 문서의 목차';
    toc.append(title);
  }
  for (const h of content.querySelectorAll('h1,h2,h3,h4,h5,h6')) {
    const slug = h.textContent.trim().toLowerCase().replace(/[^\p{L}\p{N}_ -]/gu, '').replace(/ /g, '-');
    h.id = slug || 'heading-' + (++count);
    if (document.getElementById(h.id) !== h) h.id += '-' + (++count);
    const level = Number(h.tagName[1]);
    if (level > 3 || headings.length < 2) continue;
    const a = document.createElement('a');
    a.href = '#' + encodeURIComponent(h.id);
    a.textContent = h.textContent;
    a.style.paddingLeft = (6 + (level - 1) * 12) + 'px';
    toc.append(a);
  }

  document.querySelector('#toggle').onclick = () => {
    const source = document.querySelector('#source');
    source.hidden = !source.hidden;
    content.hidden = !source.hidden;
    toc.hidden = !source.hidden;
    document.querySelector('#toggle').textContent = source.hidden ? '원문 보기' : '미리보기';
  };
  if (location.hash) {
    const target = document.getElementById(decodeURIComponent(location.hash.slice(1)));
    if (target) target.scrollIntoView();
  }
}
