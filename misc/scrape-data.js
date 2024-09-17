/**
 * @fileoverview Scrape ranking data
 */

// Get the table
const table = document.querySelector('table[class~="rankings-table"]');

// Get the head
const head = table.querySelector('tr:not(:has(td))');

// Get the rows
const rows = table.querySelectorAll('tr:has(td)');

const data = [];

// Get the labels
const labels = ['team'];
for (const th of head.querySelectorAll('th:has(a)'))
{
  let raw = th.textContent.trim();

  if (th.classList.contains('bg-primary'))
  {
    raw = `*${raw}*`;
  }

  labels.push(raw);
}
data.push(labels);

// Get the rankings
for (const row of rows)
{
  const ranking = [row.querySelector('td[class~="truncate-small"]').textContent.trim()];
  for (const cell of row.querySelectorAll('td > span, td[class~="bg-warning"]'))
  {
    // Get the value
    const raw = cell.textContent.trim();
    const value = isNaN(parseInt(raw)) ? 77 : parseInt(raw);

    // Add to the ranking
    ranking.push(value);
  }

  // Add to the rankings
  data.push(ranking);
}

// Output the data
console.log(data.map(row => row.map(cell => JSON.stringify(cell)).join(', ')).join('\n'));
