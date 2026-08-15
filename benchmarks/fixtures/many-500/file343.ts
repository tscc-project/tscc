interface Item343 { id: number; name: string; }
type Key343 = string | number;
export function make343(id: number, name: string): Item343 {
  const value: Item343 = {id, name};
  return value as Item343;
}
export const item343: Item343 = make343(343, "item-343");
