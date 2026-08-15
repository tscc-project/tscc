interface Item375 { id: number; name: string; }
type Key375 = string | number;
export function make375(id: number, name: string): Item375 {
  const value: Item375 = {id, name};
  return value as Item375;
}
export const item375: Item375 = make375(375, "item-375");
