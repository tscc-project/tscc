interface Item83 { id: number; name: string; }
type Key83 = string | number;
export function make83(id: number, name: string): Item83 {
  const value: Item83 = {id, name};
  return value as Item83;
}
export const item83: Item83 = make83(83, "item-83");
