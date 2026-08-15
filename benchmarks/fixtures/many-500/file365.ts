interface Item365 { id: number; name: string; }
type Key365 = string | number;
export function make365(id: number, name: string): Item365 {
  const value: Item365 = {id, name};
  return value as Item365;
}
export const item365: Item365 = make365(365, "item-365");
