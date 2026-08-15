interface Item413 { id: number; name: string; }
type Key413 = string | number;
export function make413(id: number, name: string): Item413 {
  const value: Item413 = {id, name};
  return value as Item413;
}
export const item413: Item413 = make413(413, "item-413");
