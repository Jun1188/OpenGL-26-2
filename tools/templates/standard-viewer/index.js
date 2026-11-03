// 분류별 문서 목록. 데이터는 documents.js(STANDARD_DOCUMENTS, STANDARD_CATALOG)를 읽는다.
const records = window.STANDARD_DOCUMENTS;
const catalog = window.STANDARD_CATALOG;
const byPath = new Map(records.map(d => [d.path, d]));
const hrefOf = d => d.markdown === null ? d.path : 'document.html?doc=' + encodeURIComponent(d.path);

function card(entry) {
  const d = byPath.get(entry.path);
  const a = document.createElement('a');
  a.className = 'card';
  a.href = hrefOf(d);
  const title = document.createElement('span');
  title.className = 'title';
  title.textContent = entry.title || d.title;
  const badge = document.createElement('span');
  badge.className = 'badge' + (d.markdown === null ? ' html' : '');
  badge.textContent = d.markdown === null ? 'HTML' : 'MD';
  title.append(badge);
  a.append(title);
  if (entry.summary) {
    const summary = document.createElement('span');
    summary.className = 'summary';
    summary.textContent = entry.summary;
    a.append(summary);
  }
  const path = document.createElement('span');
  path.className = 'path';
  path.textContent = d.path;
  a.append(path);
  return a;
}

function renderFeatured() {
  const box = document.querySelector('#featured');
  for (const category of catalog.categories)
    for (const entry of category.documents) {
      if (!entry.featured) continue;
      const d = byPath.get(entry.path);
      const a = document.createElement('a');
      a.href = hrefOf(d);
      const strong = document.createElement('strong');
      strong.textContent = entry.title || d.title;
      const span = document.createElement('span');
      span.textContent = entry.summary || d.path;
      a.append(strong, span);
      box.append(a);
    }
  if (!box.children.length) box.remove();
}

function render() {
  const query = document.querySelector('#search').value.trim().toLocaleLowerCase();
  const kind = document.querySelector('#kind').value;
  const filtering = query !== '' || kind !== 'all';
  const list = document.querySelector('#list');
  const chips = document.querySelector('#chips');
  list.replaceChildren();
  chips.replaceChildren();
  let shown = 0;
  for (const category of catalog.categories) {
    const entries = category.documents.filter(entry => {
      const d = byPath.get(entry.path);
      const text = ((entry.title || '') + ' ' + d.title + ' ' + d.path + ' ' + (entry.summary || '')).toLocaleLowerCase();
      return text.includes(query) && (kind === 'all' || (kind === 'md') === (d.markdown !== null));
    });
    if (!entries.length) continue;
    shown += entries.length;
    const section = document.createElement('section');
    section.className = 'category';
    section.id = 'cat-' + category.id;
    const h2 = document.createElement('h2');
    h2.textContent = category.title;
    const small = document.createElement('small');
    small.textContent = entries.length + '개';
    h2.append(small);
    const p = document.createElement('p');
    p.textContent = category.description;
    const cards = document.createElement('div');
    cards.className = 'cards';
    for (const entry of entries) cards.append(card(entry));
    if (category.collapsed && !filtering) {
      const details = document.createElement('details');
      const summary = document.createElement('summary');
      summary.textContent = '펼쳐 보기';
      details.append(summary, cards);
      section.append(h2, p, details);
    } else section.append(h2, p, cards);
    list.append(section);
    const chip = document.createElement('a');
    chip.href = '#' + section.id;
    chip.textContent = category.title + ' ' + entries.length;
    chips.append(chip);
  }
  if (!shown) {
    const empty = document.createElement('p');
    empty.className = 'empty';
    empty.textContent = '검색 결과가 없습니다.';
    list.append(empty);
  }
  document.querySelector('#count').textContent = filtering
    ? shown + ' / ' + records.length + '개 문서가 검색됨'
    : '전체 ' + records.length + '개 문서 · ' + catalog.categories.length + '개 분류';
}

renderFeatured();
document.querySelector('#search').addEventListener('input', render);
document.querySelector('#kind').addEventListener('change', render);
render();
