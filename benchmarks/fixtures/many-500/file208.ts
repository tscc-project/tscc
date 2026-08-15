interface Item208 { id: number; name: string; }
type Key208 = string | number;
export function make208(id: number, name: string): Item208 {
  const value: Item208 = {id, name};
  return value as Item208;
}
export const item208: Item208 = make208(208, "item-208");
