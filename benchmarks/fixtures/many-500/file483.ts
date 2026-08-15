interface Item483 { id: number; name: string; }
type Key483 = string | number;
export function make483(id: number, name: string): Item483 {
  const value: Item483 = {id, name};
  return value as Item483;
}
export const item483: Item483 = make483(483, "item-483");
