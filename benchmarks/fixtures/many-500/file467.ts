interface Item467 { id: number; name: string; }
type Key467 = string | number;
export function make467(id: number, name: string): Item467 {
  const value: Item467 = {id, name};
  return value as Item467;
}
export const item467: Item467 = make467(467, "item-467");
