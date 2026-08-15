interface Item433 { id: number; name: string; }
type Key433 = string | number;
export function make433(id: number, name: string): Item433 {
  const value: Item433 = {id, name};
  return value as Item433;
}
export const item433: Item433 = make433(433, "item-433");
