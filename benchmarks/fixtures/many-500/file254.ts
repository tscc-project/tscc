interface Item254 { id: number; name: string; }
type Key254 = string | number;
export function make254(id: number, name: string): Item254 {
  const value: Item254 = {id, name};
  return value as Item254;
}
export const item254: Item254 = make254(254, "item-254");
