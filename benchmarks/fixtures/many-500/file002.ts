interface Item2 { id: number; name: string; }
type Key2 = string | number;
export function make2(id: number, name: string): Item2 {
  const value: Item2 = {id, name};
  return value as Item2;
}
export const item2: Item2 = make2(2, "item-2");
