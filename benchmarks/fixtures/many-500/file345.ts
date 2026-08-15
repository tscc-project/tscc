interface Item345 { id: number; name: string; }
type Key345 = string | number;
export function make345(id: number, name: string): Item345 {
  const value: Item345 = {id, name};
  return value as Item345;
}
export const item345: Item345 = make345(345, "item-345");
