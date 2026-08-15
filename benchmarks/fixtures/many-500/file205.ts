interface Item205 { id: number; name: string; }
type Key205 = string | number;
export function make205(id: number, name: string): Item205 {
  const value: Item205 = {id, name};
  return value as Item205;
}
export const item205: Item205 = make205(205, "item-205");
