interface Item169 { id: number; name: string; }
type Key169 = string | number;
export function make169(id: number, name: string): Item169 {
  const value: Item169 = {id, name};
  return value as Item169;
}
export const item169: Item169 = make169(169, "item-169");
