interface Item170 { id: number; name: string; }
type Key170 = string | number;
export function make170(id: number, name: string): Item170 {
  const value: Item170 = {id, name};
  return value as Item170;
}
export const item170: Item170 = make170(170, "item-170");
