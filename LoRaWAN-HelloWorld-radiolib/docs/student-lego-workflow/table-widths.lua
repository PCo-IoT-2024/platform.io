-- Normalize Markdown table widths for the generated PDF.
--
-- Pandoc sometimes converts pipe tables into LaTeX tables with explicit
-- paragraph-column widths. For short tables this can make them span the whole
-- text width although the content would fit naturally. This filter resets the
-- column widths to Pandoc's default/natural width for compact tables, while
-- leaving wider tables untouched so long cells can still wrap.

local stringify = pandoc.utils.stringify

local function normalized_text_width(cell)
  local text = stringify(cell or "")
  text = text:gsub("%s+", " ")
  text = text:gsub("^%s+", "")
  text = text:gsub("%s+$", "")
  return utf8.len(text) or #text
end

local function update_widths_from_row(row, widths)
  if row == nil or row.cells == nil then
    return
  end

  for index, cell in ipairs(row.cells) do
    local width = normalized_text_width(cell)
    if widths[index] == nil or width > widths[index] then
      widths[index] = width
    end
  end
end

local function collect_widths(table)
  local widths = {}

  if table.head ~= nil and table.head.rows ~= nil then
    for _, row in ipairs(table.head.rows) do
      update_widths_from_row(row, widths)
    end
  end

  if table.bodies ~= nil then
    for _, body in ipairs(table.bodies) do
      if body.head ~= nil then
        for _, row in ipairs(body.head) do
          update_widths_from_row(row, widths)
        end
      end
      if body.body ~= nil then
        for _, row in ipairs(body.body) do
          update_widths_from_row(row, widths)
        end
      end
    end
  end

  if table.foot ~= nil and table.foot.rows ~= nil then
    for _, row in ipairs(table.foot.rows) do
      update_widths_from_row(row, widths)
    end
  end

  return widths
end

local function estimated_table_width(widths)
  local total = 0
  local columns = 0

  for _, width in ipairs(widths) do
    total = total + width
    columns = columns + 1
  end

  if columns > 1 then
    total = total + (columns - 1) * 3
  end

  return total, columns
end

function Table(table)
  if table.colspecs == nil then
    return table
  end

  local widths = collect_widths(table)
  local estimated_width, columns = estimated_table_width(widths)

  -- Reset only compact tables. Wider tables keep explicit Pandoc widths so
  -- long descriptions can wrap instead of overflowing into the page margin.
  if columns > 0 and estimated_width <= 85 then
    for index, colspec in ipairs(table.colspecs) do
      table.colspecs[index] = { colspec[1], 0 }
    end
  end

  return table
end
