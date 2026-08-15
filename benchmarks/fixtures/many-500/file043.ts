interface Item43 { id: number; name: string; }
type Key43 = string | number;
export function make43(id: number, name: string): Item43 {
  const value: Item43 = {id, name};
  return value as Item43;
}
export const item43: Item43 = make43(43, "item-43");
