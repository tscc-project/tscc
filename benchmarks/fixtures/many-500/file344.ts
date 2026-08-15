interface Item344 { id: number; name: string; }
type Key344 = string | number;
export function make344(id: number, name: string): Item344 {
  const value: Item344 = {id, name};
  return value as Item344;
}
export const item344: Item344 = make344(344, "item-344");
