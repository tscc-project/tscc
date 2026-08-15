interface Item339 { id: number; name: string; }
type Key339 = string | number;
export function make339(id: number, name: string): Item339 {
  const value: Item339 = {id, name};
  return value as Item339;
}
export const item339: Item339 = make339(339, "item-339");
