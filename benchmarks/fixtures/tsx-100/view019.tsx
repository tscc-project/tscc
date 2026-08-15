interface Props19 { value: number; label: string }
export const View19 = (p: Props19) => (
  <section data-i="19">
    <h2>{p.label}</h2>
    {p.value > 19 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
