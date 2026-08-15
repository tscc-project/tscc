interface Item496 { id: number; name: string; }
type Key496 = string | number;
export function make496(id: number, name: string): Item496 {
  const value: Item496 = {id, name};
  return value as Item496;
}
export const item496: Item496 = make496(496, "item-496");
