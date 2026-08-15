interface Item310 { id: number; name: string; }
type Key310 = string | number;
export function make310(id: number, name: string): Item310 {
  const value: Item310 = {id, name};
  return value as Item310;
}
export const item310: Item310 = make310(310, "item-310");
