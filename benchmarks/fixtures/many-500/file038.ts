interface Item38 { id: number; name: string; }
type Key38 = string | number;
export function make38(id: number, name: string): Item38 {
  const value: Item38 = {id, name};
  return value as Item38;
}
export const item38: Item38 = make38(38, "item-38");
