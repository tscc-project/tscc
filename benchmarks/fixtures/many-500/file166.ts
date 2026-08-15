interface Item166 { id: number; name: string; }
type Key166 = string | number;
export function make166(id: number, name: string): Item166 {
  const value: Item166 = {id, name};
  return value as Item166;
}
export const item166: Item166 = make166(166, "item-166");
