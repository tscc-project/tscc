interface Item201 { id: number; name: string; }
type Key201 = string | number;
export function make201(id: number, name: string): Item201 {
  const value: Item201 = {id, name};
  return value as Item201;
}
export const item201: Item201 = make201(201, "item-201");
