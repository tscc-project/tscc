interface Item34 { id: number; name: string; }
type Key34 = string | number;
export function make34(id: number, name: string): Item34 {
  const value: Item34 = {id, name};
  return value as Item34;
}
export const item34: Item34 = make34(34, "item-34");
