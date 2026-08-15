interface Item392 { id: number; name: string; }
type Key392 = string | number;
export function make392(id: number, name: string): Item392 {
  const value: Item392 = {id, name};
  return value as Item392;
}
export const item392: Item392 = make392(392, "item-392");
